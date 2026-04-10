<?php

class DemoLib
{

    /*
     * Register New User
     *
     * @param $name, $email, $username, $password
     * @return ID
     * */
    public function Register($name, $email, $username, $password)
    {
        try {
            //$db = DB();
            $db = new PDO("sqlite:db/login_system.db");
            $insert = "INSERT INTO users(name, email, username, password) VALUES (:name,:email,:username,:password)";
            $query = $db->prepare($insert);
            $query->bindParam("name", $name, PDO::PARAM_STR);
            $query->bindParam("email", $email, PDO::PARAM_STR);
            $query->bindParam("username", $username, PDO::PARAM_STR);
            $enc_password = hash('sha256', $password);
            $query->bindParam("password", $enc_password, PDO::PARAM_STR);
            $query->execute();
            return $db->lastInsertId();
        } catch (PDOException $e) {
            exit($e->getMessage());
        }
    }
 
    /*
     * Check Username
     *
     * @param $username
     * @return boolean
     * */
    public function isUsername($username)
    {

        try {
            //$db = DB();
            $db = new PDO("sqlite:db/login_system.db");
            $select = "SELECT user_id FROM users WHERE username=:username";
            $query = $db->prepare($select);
            $query->bindParam("username", $username, PDO::PARAM_STR);
            $query->execute();
            if ($query->fetch() > 0) {
                return true;
            } else {
                return false;
            }
        } catch (PDOException $e) {
            exit($e->getMessage());
        }
    }
 
    /*
     * Check Email
     *
     * @param $email
     * @return boolean
     * */
    public function isEmail($email)
    {

        try {
            //$db = DB();
            $db = new PDO("sqlite:db/login_system.db");
            $select = "SELECT user_id FROM users WHERE email=:email";
            $query = $db->prepare($select);
            $query->bindParam("email", $email, PDO::PARAM_STR);
            $query->execute();
            if ($query->fetch() > 0) {
                return true;
            } else {
                return false;
            }
        } catch (PDOException $e) {
            exit($e->getMessage());
        }
    }
 
    /*
     * Login
     *
     * @param $username, $password
     * @return $mixed
     * */
    public function Login($username, $password)
    {
        try {
            //$db = DB();
            $db = new PDO("sqlite:db/login_system.db");
            $select = "SELECT user_id FROM users WHERE username=:username AND password=:password";
            //$select = "SELECT user_id FROM users WHERE username=:username AND password=:password";
            $query = $db->prepare($select);
            $query->bindParam("username", $username, PDO::PARAM_STR);
            $enc_password = hash('sha256', $password);
            $query->bindParam("password", $enc_password, PDO::PARAM_STR);
            $query->execute();
            if ($query->columnCount() > 0) {
                $result = $query->fetch(PDO::FETCH_OBJ);
                return $result->user_id;
            } else {
                return false;
            }
        } catch (PDOException $e) {
            exit($e->getMessage());
        }
    }
 
    /*
     * get User Details
     *
     * @param $user_id
     * @return $mixed
     * */
    public function UserDetails($user_id)
    {
        try {
            //$db = DB();
            $db = new PDO("sqlite:db/login_system.db");
            $select = "SELECT user_id, name, username, email FROM users WHERE user_id=:user_id";
            $query = $db->prepare($select);
            $query->bindParam("user_id", $user_id, PDO::PARAM_STR);
            $query->execute();
            if ($query->columnCount() > 0) {
                return $query->fetch(PDO::FETCH_OBJ);
            }
        } catch (PDOException $e) {
            exit($e->getMessage());
        }
    }
}

?>