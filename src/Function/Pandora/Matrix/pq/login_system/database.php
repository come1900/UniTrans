<?php

// database Connection 
function DB()
{
    try {
        $db = new PDO("sqlite:db/login_system.db");
        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        return $db;
    } catch (PDOException $e) {
        return "Error!: " . $e->getMessage();
        die();
    }
}

?>