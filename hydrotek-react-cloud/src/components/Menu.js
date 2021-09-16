import { connect } from "react-redux";
import { setRoute } from "../actions/UIActions";

class Menu extends React.Component {
  constructor(props) {
    super(props);
    this.state = {};
  }
  onDashClick = () => {
    this.props.dispatch(setRoute("dashboard"));
    this.props.menuToggle();
  };
  onDevicesClick = () => {
    this.props.dispatch(setRoute("devices"));
    this.props.menuToggle();
  };
  onSupportClick = () => {
    this.props.dispatch(setRoute("support"));
    this.props.menuToggle();
  };
  onReportsClick = () => {
    this.props.dispatch(setRoute("reports"));
    this.props.menuToggle();
  };
  render() {
    return (
      <div className="menu-container">
        <button
          onClick={this.onDashClick}
          className="button-default menu-button"
        >
          <i className="fas fa-tachometer-alt fa-menu"></i>Dashboard
        </button>
        <button
          onClick={this.onDevicesClick}
          className="button-default menu-button"
        >
          <i className="fas fa-seedling fa-menu"></i>Device List
        </button>
        <button
          onClick={this.onReportsClick}
          className="button-default menu-button"
        >
          <i className="fas fa-chart-bar fa-menu"></i>Reports
        </button>
        <button
          onClick={this.onSupportClick}
          className="button-default menu-button"
        >
          <i className="far fa-question-circle fa-menu"></i>Help / Support
        </button>
      </div>
    );
  }
}

export default connect((state) => state)(Menu);
