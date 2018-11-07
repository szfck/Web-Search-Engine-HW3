const express = require('express');
const app = express();
var fs = require('fs');

app.get('/search', function (req, res) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, Content-Type, X-Auth-Token, X-Requested-With");
  res.header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");

  var data = req.query.text + '\n';

  // write query data to in.txt file for C++ program to read and process
  fs.writeFileSync('../../in.txt', data);

  // wait 2 secs and read result from out.txt file
  // (C++ program should already writen the query result to this file)
  setTimeout(function() {
    var ans = fs.readFileSync('../../out.txt').toString().split(/\r?\n/);
    console.log(ans[0]);
    var json = [];
    for (var i = 0; i + 2 < ans.length; i += 3) {
      json.push({
        url: ans[i],
        score: ans[i + 1],
        snippet: ans[i + 2]
      });
    }

    res.send(json);
  }, 2000);
});

// server at port 3000
app.listen(3000, () => console.log('Example app listening on port 3000!'));
