const fs = require('fs');
let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
let email_user = config.email.user;
let email_pass = config.email.pass;
let code_prefix = "code_";
module.exports = {email_pass, email_user, code_prefix}