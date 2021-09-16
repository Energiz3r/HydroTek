//returns timestamp (milliseconds)
export const getNowTimestamp = () => {
  +new Date();
  return Date.now();
};

//returns friendly date string from a timestamp eg. '2018-06-24 10:37:21'
export const getFriendlyFromTimestamp = (timestamp, dateOnly = false) => {
  var a = new Date(timestamp);
  var year = a.getFullYear();
  var month =
    a.getMonth() + 1 < 10 ? "0" + (a.getMonth() + 1) : a.getMonth() + 1;
  var date = a.getDate() < 10 ? "0" + a.getDate() : a.getDate();
  var hour = a.getHours();
  var min = a.getMinutes() < 10 ? "0" + a.getMinutes() : a.getMinutes();
  var sec = a.getSeconds() < 10 ? "0" + a.getSeconds() : a.getSeconds();
  if (dateOnly) {
    var time = year + "-" + month + "-" + date;
  } else {
    var time =
      year + "-" + month + "-" + date + " " + hour + ":" + min + ":" + sec;
  }
  return time;
};

//return the difference in months
export const monthDiff = (timestamp1, timestamp2) => {
  var d1;
  var d2;
  if (timestamp1 == "now") {
    d2 = new Date();
    d1 = new Date(timestamp2);
  } else {
    var timestamp3;
    if (timestamp2 < timestamp1) {
      timestamp3 = timestamp2;
      timestamp2 = timestamp1;
      timestamp1 = timestamp3;
    }
    d1 = new Date(timestamp1);
    d2 = new Date(timestamp2);
  }
  var months;
  months = (d2.getFullYear() - d1.getFullYear()) * 12;
  months -= d1.getMonth();
  months += d2.getMonth();
  return months <= 0 ? 0 : months;
};
