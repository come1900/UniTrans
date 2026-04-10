<!DOCTYPE HTML>
<html>  
<head lang="en">  
    <meta charset="UTF-8">  
    <title>View Users</title> 

<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/css/bootstrap.min.css" integrity="sha384-PsH8R72JQ3SOdhVi3uxftmaW6Vc51MKb0q5P2rRUpPvrszuE4W1povHYgTpBfshb" crossorigin="anonymous">    
    <link rel="stylesheet" href="css/style.css">
     
</head>   
<body>  
  
<div class="signupSection">  
    <div class="info">  
        <h2>All the Users</h2>
        <h2>SQL Table</h2>
    </div>

    <div class="signupForm">
        <h2>All the Users</h2>
    <table class="table table-sm table-dark table-hover">
        <thead>
        <tr class="active">  
            <th>User Id</th>  
            <th>Name</th>  
            <th>E-mail</th>  
            <th>User Name</th> 
    <!--    <th>Pass</th> -->
            <th>Delete User</th>  
        </tr>  
        </thead>
        
  
        <?php  
        include("database.php");  

        $db = DB();

        $sql = 'SELECT user_id,name,email,username FROM users';
        foreach ($db->query($sql) as $row)
        {  
            $user_id=$row[0];  
            $name=$row[1];  
            $user_email=$row[2]; 
            $user_name=$row[3]; 
        //    $user_pass=$row[4];  
  
        ?>  
       <div style="overflow-x:auto;">
        <tbody>
        <tr class="active">  
<!--here showing results in the table -->  
            <td><?php echo $user_id;  ?></td>  
            <td><?php echo $name;  ?></td>  
            <td><?php echo $user_email;  ?></td>  
            <td><?php echo $user_name;  ?></td>
    <!--    <td><?php echo $user_pass;  ?></td> -->  
            <td><a href="delete.php?del=<?php echo $user_id ?>"><button type="button" class="btn-danger">Delete</button></a></td> <!--btn btn-danger is a bootstrap button to show danger-->  
        </tr>  
  
        <?php } ?>  
        </tbody>
       </div>
    </table>

    <ul class="noBullet">
      <li id="center-btn">
      <input type="button" id="join-btn" name="join" alt="Join" value="Log out" onclick="window.location='logout.php';">  
      </li>
    </ul>
</div> 
</div> 
</body>  
  
</html>  