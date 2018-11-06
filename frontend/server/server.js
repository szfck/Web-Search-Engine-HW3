const express = require('express');
const app = express();
var fs = require('fs');

function write(res) {
  fs.readFile('../../out.txt', function(err, buf) {
    console.log(buf.toString());
    res.send(buf.toString());
  })
}

app.get('/search', function (req, res) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, Content-Type, X-Auth-Token, X-Requested-With");
  res.header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");

  var data = req.query.text + '\n';
  console.log(data);

  fs.writeFileSync('../../in.txt', data);

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
  }, 4000);
});

app.listen(3000, () => console.log('Example app listening on port 3000!'));


