import React from 'react';
import animatedComponent from '../utils/animatedComponent'

class Information extends React.Component{
  render = () => {
    return(
      <div className='app-page'>
        <h1>Information</h1>
        <p>Hello :)</p>
      </div>
    )
  }
}

export default animatedComponent(Information)