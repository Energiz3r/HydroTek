import React from "react";
import { demoDevices } from "../utils/demo-device";

import { DashboardDeviceTile } from "./DashboardDeviceTile";

export default {
  title: "DashboardDeviceTile",
  component: DashboardDeviceTile,
  argTypes: {
    backgroundColor: { control: "color" },
  },
};

const Template = (args) => <DashboardDeviceTile {...args} />;

export const Primary = Template.bind({});
Primary.args = {
  device: demoDevices[0],
};
