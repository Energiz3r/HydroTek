import React from "react";
import PropTypes from "prop-types";
import { icon } from "../utils/iconClasses";

import { createUseStyles, useTheme } from "react-jss";
const useStyles = createUseStyles({
  icon: {
    display: "inline",
    fontSize: "3rem",
    margin: "0 1.5rem 0 1.5rem",
    textShadow: ({ theme }) => `.2rem .2rem .2rem ${theme.dropShadow}`,
  },
  iconError: {
    color: "rgb(255, 70, 70)",
  },
  lightOn: {
    textShadow: "0 0 1.4rem #ffdd38",
    color: "#ffdd38",
  },
  double: {
    textShadow: ({ theme }) =>
      `.2rem .2rem .2rem ${theme.dropShadow},
       .5rem .3rem ${theme.textDefaultColor},
       .7rem .5rem .2rem ${theme.dropShadow}`,
  },
});

export const Icon = ({
  icon,
  isError = false,
  isLightOn = false,
  isDouble = false,
}) => {
  const theme = useTheme();
  const classes = useStyles({ theme });
  return (
    <i
      className={`
        ${icon}
        ${classes.icon}
        ${isError && classes.iconError}
        ${isLightOn && classes.lightOn}
        ${isDouble && classes.double}
      `}
    ></i>
  );
};
Icon.propTypes = {
  icon: PropTypes.oneOf(Object.values(icon)).isRequired,
};
