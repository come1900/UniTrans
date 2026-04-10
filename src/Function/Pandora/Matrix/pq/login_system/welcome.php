<?php
 
// Start Session
session_start();

// check user login
if(empty($_SESSION['user_id']))
{
    header("Location: login.php");
}
 
// Database connection
require __DIR__ . '/database.php';
$db = DB();
 
// Application library ( with DemoLib class )
//require __DIR__ . '/lib/library.php';
include("lib/library.php");
$app = new DemoLib();
 
$user = $app->UserDetails($_SESSION['user_id']); // get user details
 
?>
  
<html>  
<head>  
  
    <title>  
        Registration  
    </title>  
    <link rel='stylesheet prefetch' href='https://cdnjs.cloudflare.com/ajax/libs/ionicons/2.0.1/css/ionicons.css'>
    <link rel='stylesheet prefetch' href='https://maxcdn.bootstrapcdn.com/font-awesome/4.6.3/css/font-awesome.min.css'>

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
      <li id="center-btn">
      <input type="button" id="join-btn" name="join" alt="Join" value="Users" onclick="window.location='view_users.php';">
      <input type="button" id="join-btn" name="join" alt="Join" value="Log out" onclick="window.location='logout.php';">  
      </li>
    </ul>
    </div>
      
  </div>  
</body>  
  
</html>  