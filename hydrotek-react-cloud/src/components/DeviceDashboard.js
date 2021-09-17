import { connect } from "react-redux";
import { setRoute } from "../actions/UIActions";
import ReactTooltip from "react-tooltip";
import { demoDevices } from "../utils/demo-device";
import { DashboardDeviceTile } from "./DashboardDeviceTile";

import { createUseStyles, useTheme } from "react-jss";
const useStyles = createUseStyles({
  dashbTileContainer: {
    borderRadius: "0.4rem",
    padding: "0.5rem",
    display: "grid",
    gridTemplateColumns: "repeat(4, 1fr)",
    gap: "1.5rem 1.5rem",
  },
  dashbContent: {
    width: "100%",
    maxWidth: "140rem",
    alignItems: "stretch",
    textAlign: "center",
  },
});

const DeviceDashboard = (props) => {
  const theme = useTheme();
  const classes = useStyles({ theme });
  return (
    <div className={classes.dashbContent}>
      <h2>Devices Dashboard</h2>

      <div className={classes.dashbTileContainer}>
        {demoDevices.map((device, i) => (
          <DashboardDeviceTile device={device} key={i} />
        ))}
        <DashboardDeviceTile isAddButtonOnly={true} />
      </div>

      <ReactTooltip place="top" type="dark" effect="solid" />
    </div>
  );
};

export default connect((state) => state)(DeviceDashboard);
