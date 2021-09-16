import React from "react";
import { getFriendlyFromTimestamp, monthDiff } from "../utils/dateUtils";
import { createUseStyles, ThemeProvider, useTheme } from "react-jss";

export const DashboardDeviceTile = ({ device }) => {
  return (
    <div className="dashb-device-tile-container">
      <p className="dashb-device-icon">
        <i className={"fas fa-microchip"}></i>
      </p>
      <p className="device-label-dashb">{device.deviceLabel}</p>

      <div className="tile-icon-container">
        <i className="far fa-lightbulb dashb-device-icon"></i>
        <i className="fas fa-leaf dashb-device-icon"></i>
        <i className="fas fa-thermometer-half dashb-device-icon"></i>
        <i className="fas fa-tint dashb-device-icon"></i>
        <i className="fas fa-fill-drip dashb-device-icon"></i>
        <i className="fas fa-water dashb-device-icon"></i>
        <i
          className={
            "fas fa-signal dashb-device-icon " +
            (monthDiff("now", device.lastSeenTimestamp) > 0
              ? "dashb-not-seen-month"
              : "")
          }
        ></i>
      </div>

      <p>Last Seen: {getFriendlyFromTimestamp(device.lastSeenTimestamp)}</p>
    </div>
  );
};
