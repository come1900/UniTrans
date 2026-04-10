<?php
 
// Start Session
session_start();

// check user login
if(empty($_SESSION['user_id']))
{
    //header("Location: login.php");
    $username = "Guest";
}
else
{
 
// Database connection
require __DIR__ . '/database.php';
$db = DB();
 
// Application library ( with DemoLib class )
//require __DIR__ . '/lib/library.php';
include("lib/library.php");
$app = new DemoLib();
 
$user = $app->UserDetails($_SESSION['user_id']); // get user details
    $username = $user->username;
}
?>
  
<html>  
<head>  
  
    <title>  
        About PSS  
    </title>  

    <link rel="stylesheet" href="css/style.css">

</head>  
  
<body>  
  <div class="signupSection">
    <div class="info">
    <h2>Welcome</h2>
      
    <?php  

    echo $user->username;

    ?>  
    
    </div>
    <br>
    <div class="signupForm">
    <ul class="noBullet">
    <h2>PSS:Pandora Stream Services</h2>
      <li id="center-btn">
      </li>
    </ul>
    </div>
      
  </div>  
</body>  
  
</html>  
