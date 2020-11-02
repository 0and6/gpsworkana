<?php
if(isset($_GET['lat']) && isset($_GET['lon'])) {
    echo "apagar 02";
    //echo "Ok<br>";
    //echo "lat:".$_GET['lat'] . "   lon:" . $_GET['lon'] . "<br>";
    //echo "Repeat 5s";

    $conexion = pg_connect("host=ec2-34-234-185-150.compute-1.amazonaws.com dbname=d212b7tb07aa7e user=iqxlvrecdwtpye password=0a1efa1914fdc940758ab64d009da5af3be3c8f863e5d9ad7c9674278479d2d4");
    //echo "se realizo la conexion <br>";

    $datos = array('latitud' => $_GET['lat'], 'longitud' => $_GET['lon'], 'fecha' => date_format(new DateTime("now"),"Y-m-d H:i:s"));
    //print_r($datos);
    $insertar = pg_insert($conexion, 'coordenadas', $datos);
    /*if($insertar) {
        echo "apagar";
        echo "datos insertados correctamente";
    } else {
        echo "fallo al insertar";
    }*/

    
} else {
    echo "Error";
}
?>
