<?php

$conexion = pg_connect("host=ec2-34-234-185-150.compute-1.amazonaws.com dbname=d212b7tb07aa7e user=iqxlvrecdwtpye password=0a1efa1914fdc940758ab64d009da5af3be3c8f863e5d9ad7c9674278479d2d4");

$datos = pg_query($conexion, "SELECT latitud, longitud, fecha FROM coordenadas");

echo "<table>";
while ($row = pg_fetch_row($datos)) {
    echo "<tr>";
    echo "<td>$row[0]</td>";
    echo "<td>$row[1]</td>";
    echo "<td>$row[2]</td>";
    echo "</tr>";
}

echo "</table>";
?>