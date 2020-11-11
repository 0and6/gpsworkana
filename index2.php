<?php
if(isset($_POST['lat']) && isset($_POST['lon'])) {
    echo "Ok<br>";
    /*
    echo "lat:".$_POST['lat'] . "   lon:" . $_POST['lon'] . "<br>";
    echo "Repeat 5s";

    $conexion = pg_connect("host=ec2-34-234-185-150.compute-1.amazonaws.com dbname=d212b7tb07aa7e user=iqxlvrecdwtpye password=0a1efa1914fdc940758ab64d009da5af3be3c8f863e5d9ad7c9674278479d2d4");
    echo "se realizo la conexion <br>";

    $datos = array('latitud' => $_POST['lat'], 'longitud' => $_POST['lon'], 'fecha' => date_format(new DateTime("now"),"Y-m-d H:i:s"));
    print_r($datos);
    $insertar = pg_insert($conexion, 'coordenadas', $datos);
    if($insertar) {
        echo "datos insertados correctamente";
    } else {
        echo "fallo al insertar";
    }*/


} else {
    echo "Error";
}
?>
