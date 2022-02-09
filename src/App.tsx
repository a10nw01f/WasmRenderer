import React from 'react';
import Canvas from './Components/Canvas'
import Menu from './Components/Menu'
import './App.css';

const App: React.FC = () => {
  return (
    <div className="App">
      <Canvas></Canvas>
      <Menu></Menu>
    </div>
  );
}

export default App;
