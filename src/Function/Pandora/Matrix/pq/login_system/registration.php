<?php
session_start();//session starts here 

include("database.php");//make connection here

// Application library ( with DemoLib class )
require __DIR__ . '/lib/library.php';
//include("lib/library.php");
$app = new DemoLib();
 
$login_error_message = '';
$register_error_message = '';
  
// check Register request
if (!empty($_POST['join'])) {
    if ($_POST['name'] == "") {
        $register_error_message = 'Name field is required!';
    } else if ($_POST['email'] == "") {
        $register_error_message = 'Email field is required!';
    } else if ($_POST['username'] == "") {
        $register_error_message = 'Username field is required!';
    } else if ($_POST['password'] == "") {
        $register_error_message = 'Password field is required!';
    } else if (!filter_var($_POST['email'], FILTER_VALIDATE_EMAIL)) {
        $register_error_message = 'Invalid email address!';
    } else if ($app->isEmail($_POST['email'])) {
        $register_error_message = 'Email is already in use!';
    } else if ($app->isUsername($_POST['username'])) {
        $register_error_message = 'Username is already in use!';
    } else {
        $user_id = $app->Register($_POST['name'], $_POST['email'], $_POST['username'], $_POST['password']);
        // set session and redirect user to the welcome page
        $_SESSION['user_id'] = $user_id;
        header("Location: login.php");  
    }
}

?>

<!DOCTYPE html>

<html >
<head>
  <meta charset="UTF-8">
  <title>Daily UI 001 Sign Up Form</title>
  
  
  <link rel='stylesheet prefetch' href='https://cdnjs.cloudflare.com/ajax/libs/ionicons/2.0.1/css/ionicons.css'>
  <link rel='stylesheet prefetch' href='https://maxcdn.bootstrapcdn.com/font-awesome/4.6.3/css/font-awesome.min.css'>

  <link rel="stylesheet" href="css/style.css">

</head>

<body>
  <div class="signupSection">
  <div class="info">
    <h2>Sign Up</h2>
    <i class="icon ion-ios-ionic-outline" aria-hidden="true"></i>
    <p>The Future Is Here</p>
  </div>

        <?php
            if ($register_error_message != "") {
                echo '<script language="javascript">alert("Please enter valid fields required");</script>';
            }
        ?>
  
  <form action="registration.php" method="POST" class="signupForm" name="signupform">
    
    <ul class="noBullet">
      <li>
        <label for="name"></label>
        <input type="text" class="inputFields" id="name" name="name" placeholder="Name" value="" oninput="return userNameValidation(this.value)"/>
      </li>
      <li>
        <label for="username"></label>
        <input type="text" class="inputFields" id="username" name="username" placeholder="Username" value="" oninput="return userNameValidation(this.value)" required/>
      </li>
      <li>
        <label for="password"></label>
        <input type="password" class="inputFields" id="password" name="password" placeholder="Password" value="" oninput="return passwordValidation(this.value)" required/>
      </li>
      <li>
        <label for="email"></label>
        <input type="email" class="inputFields" id="email" name="email" placeholder="Email" value="" required/>
      </li>
      <li id="center-btn">
        <input type="submit" id="join-btn" name="join" alt="Join" value="Join">
        <input type="button" id="join-btn" name="join" alt="Join" value="Already registered" onclick="window.location='login.php';">
      </li>
    </ul>
  </form>
</div>
  
    <script src="js/index.js"></script>

</body>
</html>

