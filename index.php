<?php
if(isset($_GET['lat']) && isset($_GET['lon'])) {
    echo "Ok<br>";
    echo "lat:".$_GET['lat'] . "   lon:" . $_GET['lon'] . "<br>";
    echo "Repeat 35s";
} else {
    echo "Error";
}
?>
