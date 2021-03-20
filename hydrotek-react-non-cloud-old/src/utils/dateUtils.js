//returns friendly date string from a timestamp eg. '2018-06-24 10:37:21'
export const getDatetime = () => {
  var a = new Date();
  var year = a.getFullYear();
  var month = (a.getMonth() + 1)  < 10 ? '0' + (a.getMonth() + 1) : (a.getMonth() + 1);
  var date = a.getDate() < 10 ? '0' + a.getDate() : a.getDate();
  var hour = a.getHours();
  var min = a.getMinutes() < 10 ? '0' + a.getMinutes() : a.getMinutes();
  var sec = a.getSeconds() < 10 ? '0' + a.getSeconds() : a.getSeconds();
  var time = year + '-' + month + '-' + date + ' ' + hour + ':' + min + ':' + sec;
  return time;
}

//gets the current date now
export const getDate = () => {
  var a = new Date();
  var year = a.getFullYear();
  var month = (a.getMonth() + 1)  < 10 ? '0' + (a.getMonth() + 1) : (a.getMonth() + 1);
  var date = a.getDate() < 10 ? '0' + a.getDate() : a.getDate();
  var output = year + '-' + month + '-' + date
  return output;
}

// current timestamp
export const getDateStamp = () => {
  return Date.Now();
}