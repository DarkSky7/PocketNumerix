<?
  $recipient = "pdtMgt@pnmx.com";
  $subject = $_POST['subject'];
  $message = $_POST['message'];
  $name = $_POST['name'];
  $fromAddress = $_POST['fromAddress'];
  $fromParam = "From: " . $name . "<" . $fromAddress . ">";

  mail( $recipient, $subject, $message, $fromParam );
#  header( "About.shtml" );         # this doesn't work
  $response = "The product management team appreciates your input on '" . $subject . "'.";
  echo $response;
?>
