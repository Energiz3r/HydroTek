import React from "react";
import { getFriendlyFromTimestamp, monthDiff } from "../utils/dateUtils";
import { TileIcon } from "./TileIcon";
import { icon } from "../utils/iconClasses";

import { createUseStyles, useTheme } from "react-jss";
const useStyles = createUseStyles({
  dashbTile: {
    background: ({ theme }) =>
      `radial-gradient(${theme.backgroundLighterHover},${theme.backgroundLighter})`,
    "&:hover": {
      background: ({ theme }) =>
        `radial-gradient(${theme.backgroundEvenLighterHover},${theme.backgroundLighter})`,
      boxShadow: ".5rem .5rem .8rem .2rem rgba(20,20,20,0.5)",
    },
    borderRadius: ({ theme }) => theme.borderRadius,
    borderLeft: ({ theme }) => `0.3rem solid ${theme.highlight}`,
    height: "35rem",
    boxShadow: ".2rem .2rem .4rem .2rem rgba(20,20,20,0.2)",
    display: "grid",
  },
  addContainer: {
    alignSelf: "center",
    justifySelf: "center",
    paddingBottom: "3rem",
  },
  addButton: {
    fontSize: "15rem",
    fontWeight: "bold",

    color: "transparent",
    textShadow: ({ theme }) => `2px 2px 5px ${theme.background}`,
    WebkitBackgroundClip: "text",
    backgroundClip: "text",
    backgroundColor: "black",
    "&:hover": {
      color: ({ theme }) => theme.textDefaultColor,
      textShadow: ({ theme }) => `2px 2px 5px ${theme.background}`,
      WebkitBackgroundClip: "border-box",
      backgroundClip: "border-box",
      backgroundColor: "rgba(0,0,0,0)",
      cursor: "pointer",
    },
  },
  device: {
    cursor: "pointer",
  },
  iconContainer: {
    display: "grid",
    gap: "3rem 1.5rem",
    gridAutoFlow: "row",
    gridTemplateColumns: "repeat(2, 1fr)",
    justifyItems: "start",
  },
  deviceLabel: {
    fontSize: "2.4rem",
    padding: ".6rem",
    margin: "2rem 0 2.0rem 0",
    // backgroundColor: ({ theme }) => theme.backgroundDarker,
    // boxShadow: "inset 0 .2rem .5rem #000000",
  },
  lastSeenContainer: {
    gridColumn: "1 / span 2",
    justifySelf: "center",
  },
});

export const DashboardDeviceTile = ({ device, isAddButtonOnly }) => {
  const theme = useTheme();
  const classes = useStyles({ theme });
  return (
    <div className={classes.dashbTile}>
      {isAddButtonOnly ? (
        <div className={classes.addContainer}>
          <span className={classes.addButton}>+</span>
        </div>
      ) : (
        <div className={classes.device}>
          <h2 className={classes.deviceLabel}>{device.deviceLabel}</h2>

          <div className={classes.iconContainer}>
            <TileIcon
              icon={icon.leaf}
              label={`${device.devicePlants.length} ${
                device.devicePlants.length > 1 ? "plants" : "plant"
              }`}
              isDouble={device.devicePlants.length > 1}
            />
            <TileIcon
              icon={icon.lightbulb}
              isLightOn={device.lampIsOn}
              label={`OK${device.lampIsOn ? ", ON" : ""}`}
            />
            <TileIcon icon={icon.thermometer} label={"22.5°C"} />
            <TileIcon icon={icon.droplet} label={"OK"} />
            <TileIcon
              icon={icon.liquid}
              label={device.wasteTankFull ? "FULL" : "OK"}
              isError={device.wasteTankFull}
            />
            <TileIcon
              icon={icon.water}
              label={`${
                Math.round((Math.random() * (6.2 - 2.5) + 2.5) * 10) / 10
              }L/day`}
            />
            <TileIcon
              icon={icon.signal}
              isError={monthDiff("now", device.lastSeenTimestamp) > 0}
              label={
                monthDiff("now", device.lastSeenTimestamp) > 0
                  ? "Not Online"
                  : "Online"
              }
            />
            <div className={classes.lastSeenContainer}>
              Last Seen: {getFriendlyFromTimestamp(device.lastSeenTimestamp)}
            </div>
          </div>
        </div>
      )}
    </div>
  );
};
