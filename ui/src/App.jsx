import { useState } from 'react';

const waveformLabels = ['Sine', 'Saw', 'Square', 'Triangle'];

function App() {
  const [frequency, setFrequency] = useState(440);
  const [gain, setGain] = useState(0.3);
  const [attack, setAttack] = useState(0.01);
  const [release, setRelease] = useState(0.2);
  const [waveform, setWaveform] = useState(0);

  const sendControl = (name, value) => {
    console.log(`send ${name} = ${value}`);
    // Replace with your native bridge / host integration
  };

  return (
    <div className="app-shell">
      <h1>React Synth UI</h1>
      <div className="control-grid">
        <label>
          Frequency
          <input
            type="range"
            min="20"
            max="2000"
            value={frequency}
            onChange={(e) => {
              const value = Number(e.target.value);
              setFrequency(value);
              sendControl('frequency', value);
            }}
          />
          <span>{frequency} Hz</span>
        </label>

        <label>
          Gain
          <input
            type="range"
            min="0"
            max="1"
            step="0.01"
            value={gain}
            onChange={(e) => {
              const value = Number(e.target.value);
              setGain(value);
              sendControl('gain', value);
            }}
          />
          <span>{gain.toFixed(2)}</span>
        </label>

        <label>
          Attack
          <input
            type="range"
            min="0.001"
            max="1"
            step="0.001"
            value={attack}
            onChange={(e) => {
              const value = Number(e.target.value);
              setAttack(value);
              sendControl('attack', value);
            }}
          />
          <span>{attack.toFixed(3)} s</span>
        </label>

        <label>
          Release
          <input
            type="range"
            min="0.001"
            max="2"
            step="0.001"
            value={release}
            onChange={(e) => {
              const value = Number(e.target.value);
              setRelease(value);
              sendControl('release', value);
            }}
          />
          <span>{release.toFixed(3)} s</span>
        </label>

        <label>
          Waveform
          <select
            value={waveform}
            onChange={(e) => {
              const value = Number(e.target.value);
              setWaveform(value);
              sendControl('waveform', value);
            }}
          >
            {waveformLabels.map((label, index) => (
              <option key={label} value={index}>
                {label}
              </option>
            ))}
          </select>
        </label>
      </div>
    </div>
  );
}

export default App;
