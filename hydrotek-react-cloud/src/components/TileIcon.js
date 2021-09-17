import React from "react";
import PropTypes from "prop-types";
import { Icon } from "./Icon";

import { createUseStyles, useTheme } from "react-jss";
const useStyles = createUseStyles({
  container: {
    display: "grid",
    gap: "0 1rem",
    gridTemplateColumns: "7rem 1fr",
    alignItems: "center",
  },
  subcontainer: {
    justifySelf: "center",
  },
});

export const TileIcon = ({
  icon,
  isError = false,
  label,
  isLightOn = false,
  isDouble = false,
}) => {
  const theme = useTheme();
  const classes = useStyles({ theme });
  return (
    <div className={classes.container}>
      <div className={classes.subcontainer}>
        <Icon
          icon={icon}
          isError={isError}
          isLightOn={isLightOn}
          isDouble={isDouble}
        />
      </div>
      <div>
        <span>{label}</span>
      </div>
    </div>
  );
};

TileIcon.propTypes = {};
