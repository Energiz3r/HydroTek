
//returns timestamp (milliseconds)
export const getNowTimestamp = () => { 
  +new Date;
  return Date.now();
}

//returns friendly date string from a timestamp eg. '2018-06-24 10:37:21'
export const getFriendlyFromTimestamp = (timestamp, format) => {
  var a = new Date(timestamp);
  var year = a.getFullYear();
  var month = (a.getMonth() + 1)  < 10 ? '0' + (a.getMonth() + 1) : (a.getMonth() + 1);
  var date = a.getDate() < 10 ? '0' + a.getDate() : a.getDate();
  var hour = a.getHours();
  var min = a.getMinutes() < 10 ? '0' + a.getMinutes() : a.getMinutes();
  var sec = a.getSeconds() < 10 ? '0' + a.getSeconds() : a.getSeconds();
  var time = year + '-' + month + '-' + date + ' ' + hour + ':' + min + ':' + sec;
  return time;
}