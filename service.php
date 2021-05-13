
<?php
    try{
        $servername = "localhost";
        $database = "ddb169277" ;
        $username = "root";
        $password = "";
        
        // Creem la connexió a la BBDD
        $conn = new mysqli($servername, $username, $password, $database);
        
        // Verifiquem si la connexió és correcte
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }else{
            // Si hi ha connexió amb la BBDD recollim el valor JSON decodificat
            header("Content-type: application/json; charset=utf-8");
            $input = json_decode(file_get_contents("php://input"), true); 

            // Guardem la "clau" doncs serà el valor del nom de la nostra taula a la BBDD
            foreach($input as $key => $value) $clau = $key;

            // Verifiquem quina dada ens està entrant per l'input
            if ($input["fsr"] ) $value = ($input["fsr"] ? (int) $input["fsr"] : '' );
            if ($input["ntc"] ) $value = ($input["ntc"] ? (int) $input["ntc"] : '' );
            if ($input["bpm"] ) $value = ($input["bpm"] ? (int) $input["bpm"] : '' );
            if ($input["spo2"] ) $value = ($input["spo2"] ? (int) $input["spo2"] : '' );
            if ($input["adx"] )  $value = explode(" ",$input["adx"]);

            // Creacio de la query amb el valor a introduir a la BBDD 
            $sql = ($input["adx"] ? "INSERT INTO $clau (adx_x, adx_y, adx_z) VALUES (".$value[1].",".$value[2].",".$value[3].");" : "INSERT INTO $clau (valor) VALUES (".$value.");");

            // Guardem el valor a la BBDD
            if ($conn->query($sql)) echo json_encode("Dada OK", 200);
            else echo json_encode("Error SQL", 500);

            // Tanquem la connexió a la BBDD
            $conn->close();
        }

    }catch (Exception $e) {
        echo 'Excepción capturada: ',  $e->getMessage(), "\n";
    }
?>