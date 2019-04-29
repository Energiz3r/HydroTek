import React from 'react'
import animatedComponent from '../utils/animatedComponent'
import {setLocation} from '../actions/uiActions'
import {connect} from 'react-redux'

import CanvasJSReact from '../utils/canvasjs-2.3.1/canvasjs.react'
var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

class Charts extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      userName: window.username,
      displayMessage: window.displayMessage,
      filter: 'all'
    }
  }
  
  getFilteredData = (plant) => {
    let filterObj;
    if (plant == 1) {
      filterObj = {...window.plant1Data}
    } else {
      filterObj = {...window.plant2Data}
    }

    //figure out how to filter
    const hours = 60 * 60 * 1000
    let filterBy = 365 * 24 * hours //max filter range is last 12 months
    if (this.state.filter == 'last24') {
      filterBy = 24 * hours
    } else if (this.state.filter == 'last7days') {
      filterBy = 7 * 24 * hours
    } else if (this.state.filter == 'last30days') {
      filterBy = 30 * 24 * hours
    }

    const now = Date.now()

    //apply display filters
    filterObj.temp = filterObj.temp.filter(
      val=>val.x > now - filterBy
    )
    filterObj.humidity = filterObj.humidity.filter(
      val=>val.x > now - filterBy
    )
    filterObj.flow = filterObj.flow.filter(
      val=>val.x > now - filterBy
    )
    
    return filterObj
    
  }
  render() {
    const plant1 = {
      theme: "light2",
      animationEnabled: true,
      exportEnabled: true,
      title: {
        text: "Plant 1"
      },
      axisY: {
        title: "Values"
      },
      axisX: {
        valueFormatString: "YYYY-MM-DD hh:mm:ss tt"
      },
      toolTip: {
        shared: true
      },
      legend: {
        verticalAlign: "center",
        horizontalAlign: "right",
        reversed: true,
        cursor: "pointer"
      },
      data: [
        {
          type: "area",
          name: "Temp",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(1).temp
        },
        {
          type: "area",
          name: "Humidity",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(1).humidity
        },
        {
          type: "area",
          name: "Flow",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(1).flow
        }
      ]
    }
    const plant2 = {
      theme: "light2",
      animationEnabled: true,
      exportEnabled: true,
      title: {
        text: "Plant 2"
      },
      axisY: {
        title: "Values"
      },
      axisX: {
        valueFormatString: "YYYY-MM-DD hh:mm:ss tt"
      },
      toolTip: {
        shared: true
      },
      legend: {
        verticalAlign: "center",
        horizontalAlign: "right",
        reversed: true,
        cursor: "pointer"
      },
      data: [
        {
          type: "area",
          name: "Flow",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(2).flow
        },
        {
          type: "area",
          name: "Humidity",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(2).humidity
        },
        {
          type: "area",
          name: "Temp",
          showInLegend: true,
          xValueType: "dateTime",
          dataPoints: this.getFilteredData(2).temp
        }
      ]
    }
    return (
      <div className='main-menu-container'>
        <h2>Data Charts</h2>
        <p>{this.state.displayMessage == "" ? ("Hello, " + this.state.userName + ".") : this.state.displayMessage}</p>
        <div className="button-container">
          <p><button className={'button-base ' + (this.state.filter == 'all' ? 'button-selected' : '')} onClick={() => { this.setState({...this.state, filter: 'all'}) }} >All Time</button>
          <button className={'button-base ' + (this.state.filter == 'last24' ? 'button-selected' : '')} onClick={() => { this.setState({...this.state, filter: 'last24'}) }} >Last 24hrs</button>
          <button className={'button-base ' + (this.state.filter == 'last7days' ? 'button-selected' : '')} onClick={() => { this.setState({...this.state, filter: 'last7days'}) }} >Last 7 days</button>
          <button className={'button-base ' + (this.state.filter == 'last30days' ? 'button-selected' : '')} onClick={() => { this.setState({...this.state, filter: 'last30days'}) }} >Last 30 days</button></p>
        </div>
        <div>
			    <CanvasJSChart options = {plant1} />
          <CanvasJSChart options = {plant2} />
		    </div>
      </div>
    )
  }
}

export default animatedComponent(connect(state=>state)(Charts))