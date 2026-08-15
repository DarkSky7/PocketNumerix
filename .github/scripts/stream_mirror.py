#!/usr/bin/env python3
"""
Stream PocketNumerix from SourceForge directly into this GitHub repo via the
Contents API.  No local persistent storage is used: each file is streamed from
SourceForge into memory, base64-encoded, and written to the repo with the
auto-scoped GITHUB_TOKEN (which has contents:write on this repo).

Only files missing from the current tree are uploaded.
Set DRY_RUN=true to list what would be uploaded without writing anything.
"""
import base64, json, os, urllib.request, urllib.error, time

SF_HOST = "https://sourceforge.net"
SF_BASE = "https://sourceforge.net/projects/pocketnumerix/files/"
REPO    = os.environ.get("REPO", "DarkSky7/PocketNumerix")
TOKEN   = os.environ.get("GH_TOKEN", "")   # GH Actions GITHUB_TOKEN
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
            log(f"  ! fetch retry {a+1}/{tries} {url}: {e}")
            time.sleep(4)
    raise RuntimeError("failed to fetch " + url)

def list_dir(url):
    html = fetch(url).decode("utf-8", "replace")
    i = html.find("net.sf.files = {")
    if i < 0:
        raise RuntimeError("no net.sf.files on " + url)
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
    url = f"https://api.github.com/repos/{REPO}/contents/{path}"
    data = json.dumps({"message": f"Mirror from SourceForge PocketNumerix/{path}",
                       "content": b64, "branch": BRANCH}).encode()
    req = urllib.request.Request(url, data=data, method="PUT",
                                 headers={"Authorization": "Bearer " + TOKEN,
                                          "Accept": "application/vnd.github+json",
                                          "User-Agent": "StreamMirror"})
    for a in range(4):
        try:
            return urllib.request.urlopen(req, timeout=180).status
        except urllib.error.HTTPError as e:
            if e.code == 409:   # concurrent write conflict -> retry
                time.sleep(2); continue
            log(f"  ! PUT {path} -> HTTP {e.code}")
            return e.code
        except Exception as e:
            log(f"  ! PUT retry {a}: {e}"); time.sleep(3)
    return -1

def main():
    log(f"DRY_RUN={DRY}  REPO={REPO}  BRANCH={BRANCH}  token_set={bool(TOKEN)}")
    # 1) enumerate full SF tree
    remote = {}
    stack = [(SF_BASE, "")]; seen = set()
    while stack:
        url, _ = stack.pop()
        if url in seen: continue
        seen.add(url)
        try:
            fl, dr = list_dir(url)
        except Exception as e:
            log(f"  ! listing {url}: {e} -- skipping"); continue
        for f in fl: remote[f["full_path"]] = f
        for d in dr: stack.append((d["url"], d["full_path"]))
    log(f"SF tree: {len(remote)} files")

    # 2) current tracked tree (from checkout)
    tracked = set()
    for root, _, fs in os.walk("."):
        if "/.git" in root.replace("\\", "/"): continue
        for fn in fs:
            p = os.path.relpath(os.path.join(root, fn)).replace("\\", "/")
            tracked.add(p)
