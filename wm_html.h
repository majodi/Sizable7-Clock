const char wifimanager_html[] = R"=="==(
<!DOCTYPE html>
<html>
<head>
  <title>S7 Wi-Fi Manager</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
<style>
html {
  font-family: Arial, Helvetica, sans-serif; 
  display: inline-block; 
  text-align: center;
}
h1 {
  font-size: 1.8rem; 
  color: white;
}
p { 
  font-size: 1.4rem;
}
.topnav { 
  overflow: hidden; 
  background-color: #0A1128;
}
body {  
  margin: 0;
}
.content { 
  padding: 50px;
}
.card-grid { 
  max-width: 800px; 
  margin: 0 auto; 
  display: grid; 
  grid-gap: 2rem; 
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
}
.card { 
  background-color: white; 
  box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
}
.card-title { 
  font-size: 1.2rem;
  font-weight: bold;
  color: #034078
}
</style>
</head>
<body>
  <div class="topnav">
    <h1>S7 Clock Wi-Fi Manager</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <form action="/" method="POST">
          <p>
            <label for="ssid">SSID</label>
            <input type="text" autocorrect="off" autocapitalize="none" id="ssid" name="ssid"><br>
            <label for="pass">Password</label>
            <input type="text" autocorrect="off" autocapitalize="none" id="pass" name="pass"><br>
            <input type ="submit" value="Submit">
          </p>
        </form>
      </div>
    </div>
  </div>
</body>
</html>
)=="==";