<?
  $recipient = "webmaster@pnmx.com";
  $subject = $_POST['subject'];
  $message = $_POST['message'];
  $name = $_POST['name'];
  $fromAddress = $_POST['fromAddress'];
  $fromParam = "From: " . $name . "<" . $fromAddress . ">";

  mail( $recipient, $subject, $message, $fromParam );
#  header( "Loation: About.shtml" );         # this doesn't work
  $response = "Thanks for your input on '" . $subject . "' ... the Webmaster";
  echo $response;
?>
