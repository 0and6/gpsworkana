<?php
if(isset($_GET['lat']) && isset($_GET['lon'])) {
    echo "Ok<br>";
    echo "lat:".$_GET['lat'] . "   lon:" . $_GET['lon'] . "<br>";
    echo "Repeat35s";
    
    $conexion = pg_connect("host=ec2-34-234-185-150.compute-1.amazonaws.com dbname=d212b7tb07aa7e user=iqxlvrecdwtpye password=0a1efa1914fdc940758ab64d009da5af3be3c8f863e5d9ad7c9674278479d2d4");

    pg_insert($conexion, 'coordenadas', $_GET);
    
} else {
    echo "Error";
}
?>
