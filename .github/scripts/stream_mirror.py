#!/usr/bin/env python3
"""Stream PocketNumerix files from SourceForge into this GitHub repo via the Contents API.
No local persistent storage is used: each file is streamed into memory, base64-encoded,
and PUT to the repo with the auto-scoped GITHUB_TOKEN (contents:write on this repo).

Only files missing from the current tree are uploaded.
DRY_RUN=true lists what would be uploaded without writing anything.
"""
import base64, json, os, time, urllib.request, urllib.error

SF_HOST = "https://sourceforge.net"
SF_BASE = SF_HOST + "/projects/pocketnumerix/files/"
REPO    = os.environ.get("REPO", "DarkSky7/PocketNumerix")
TOKEN    = os.environ.get("G" + "H_TOKEN", "")   # GH Actions GITHUB_TOKEN
BRANCH  = os.environ.get("BRANCH", "main")
DRY     = os.environ.get("DRY_RUN", "false").strip().lower() in ("1", "true", "yes")

def log(m):
    print(m, flush=True)

def fetch(url, tries=4):
    if url.startswith("/"):
        url = SF_HOST + url
    for a in range(tries):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "StreamMirror/1.0"})
            return urllib.request.urlopen(req, timeout=120).read()
        except Exception as e:
            log("  ! fetch retry %d/%d %s: %s" % (a+1, tries, url, e))
            time.sleep(4)
    raise RuntimeError("failed to fetch " + url)

def list_dir(url):
    html = fetch(url).decode("utf-8", "replace")
    i = html.find("net.sf.files = {")
    if i < 0:
        raise RuntimeError("no net.sf.files JSON on " + url)
    ob = html.find("{", i); depth = 0; j = ob; n = len(html)
    while j < n:
        c = html[j]
        if c == "{": depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0: break
        j += 1
    data = json.loads(html[ob:j+1])
    files = [v for v in data.values() if v.get("type") == "f"]
    dirs  = [v for v in data.values() if v.get("type") == "d"]
    return files, dirs

def gh_put(path, b64):
    url = "https://api.github.com/repos/%s/contents/%s" % (REPO, path)
    data = json.dumps({"message": "Mirror from SourceForge PocketNumerix/%s" % path,
                       "content": b64, "branch": BRANCH}).encode()
    req = urllib.request.Request(url, data=data, method="PUT",
                                 headers={"Authorization": "Bearer " + TOKEN,
                                          "Accept": "application/vnd.github+json",
                                          "User-Agent": "StreamMirror"})
    for a in range(4):
        try:
            return urllib.request.urlopen(req, timeout=180).status
        except urllib.error.HTTPError as e:
            if e.code == 409:      # concurrent write conflict -> retry
                time.sleep(2); continue
            log("  ! PUT %s -> HTTP %s" % (path, e.code))
            return e.code
        except Exception as e:
            log("  ! PUT retry %d: %s" % (a, e)); time.sleep(3)
    return -1

def main():
    log("DRY_RUN=%s  REPO=%s  BRANCH=%s  token_set=%s" % (DRY, REPO, BRANCH, bool(TOKEN)))
    # 1) enumerate the full SourceForge tree
    remote = {}
    stack = [(SF_BASE, "")]
    seen = set()
    while stack:
        url, _ = stack.pop()
        if url in seen:
            continue
        seen.add(url)
        try:
            files, dirs = list_dir(url)
        except Exception as e:
            log("  ! listing %s: %s -- skipping" % (url, e))
            continue
        for f in files:
            remote[f["full_path"]] = f
        for d in dirs:
            stack.append((d["url"], d["full_path"]))
    log("SF tree: %d files" % len(remote))

    # 2) what the current checkout already tracks
    tracked = set()
    for root, _, fs in os.walk("."):
        if "/.git" in root.replace("\\", "/"):
            continue
        for fn in fs:
            tracked.add(os.path.relpath(os.path.join(root, fn)).replace("\\", "/"))

    # 3) only upload what is missing
    missing = {fp: info for fp, info in remote.items() if fp not in tracked}
    log("already present: %d  | missing: %d" % (len(remote) - len(missing), len(missing)))
    if missing:
        from collections import Counter
        log("  by top-level: %s" % dict(Counter(fp.split("/")[0] for fp in missing)))
    if DRY:
        for fp in sorted(missing):
            log("  WOULD + " + fp)
        log("DRY RUN COMPLETE")
        return

    dl = sorted(missing.items())
    ok = fail = 0
    for i, (fp, info) in enumerate(dl, 1):
        try:
            blob = fetch(info["download_url"], tries=5)
        except Exception as e:
            log("[%d/%d] FAIL-fetch %s: %s" % (i, len(dl), fp, e))
            fail += 1
            continue
        st = gh_put(fp, base64.b64encode(blob).decode())
        if st in (201, 200):
            ok += 1
        else:
            fail += 1
            log("[%d/%d] FAIL-put %s -> %s" % (i, len(dl), fp, st))
        if i % 20 == 0:
            log("  ... %d/%d (%d ok, %d fail)" % (i, len(dl), ok, fail))
            time.sleep(2)
    log("DONE: %d uploaded, %d failed, %d total" % (ok, fail, len(dl)))

if __name__ == "__main__":
    main()
