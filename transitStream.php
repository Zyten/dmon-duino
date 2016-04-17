<?php
 
$temp= 0;
$humidity= 0;
$dust= 0;

$temp= $_GET['temp'];
if($temp == 0) {
  echo '{"temp":"0"}';
  exit(0);
}
echo '{"temp":"'.$temp.'"}';

$humidity= $_GET['humidity'];
if($humidity== 0) {
  echo '{"humidity":"0"}';
  exit(0);
}
echo '{"humidity":"'.$humidity.'"}';

$dust= $_GET['dust'];
if($dust== 0) {
  echo '{"dust":"0"}';
  exit(0);
}
echo '{"dust":"'.$dust.'"}';

$data = array("protocol"=>"v2", //Compose JSON stream
			  "at"=> "now",
			  "device"=>"D-MON@zytenn.zytenn",
			  "data" => array ("temperature" => trim($temp), "humidity" => trim($humidity), "dust_density" => trim($dust)),
			  "checksum" => "");
$data_string = json_encode($data);

$ch = curl_init('https://api.carriots.com/status'); //Compose the HTTP request                                                                  
curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, array(
	'Content-Type: application/json',
	'carriots.apikey: f88617251146351ca66cad2bd0945f3149c65169eb1cf3a16e13604df7b9ffc5',
	'Content-Length: ' . strlen($data_string))
);       

$result = curl_exec($ch); //Send the stream
if (!result)
  {echo '{"transit":"FAILED"}';}
else
  {echo '{"transit":"SUCCESS"}';}
?>					