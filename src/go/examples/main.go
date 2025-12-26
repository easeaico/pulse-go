package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/easeaico/pulse-go/pulse"
)

func main() {
	fmt.Println("Pulse Go Binding Example")
	fmt.Println("========================")

	// Print Build Information
	fmt.Printf("Pulse Version: %s\n", pulse.Version())
	fmt.Printf("Pulse Hash: %s\n", pulse.Hash())

	// Get the data directory (relative to executable location)
	execPath, err := os.Executable()
	if err != nil {
		fmt.Printf("Failed to get executable path: %v\n", err)
		return
	}
	// Assume data directory is at project root/data
	dataDir := filepath.Join(filepath.Dir(execPath), "..", "..", "..", "..", "data")
	if _, err := os.Stat(dataDir); os.IsNotExist(err) {
		// Try current working directory
		cwd, _ := os.Getwd()
		dataDir = filepath.Join(cwd, "..", "..", "..", "data")
	}
	fmt.Printf("Data Directory: %s\n\n", dataDir)

	// Initialize the Pulse library
	pulse.Initialize()
	defer pulse.Deinitialize()

	// Create a new engine instance
	pe := pulse.NewPulseEngine(pulse.HumanAdultWholeBody, dataDir)
	if pe == nil {
		fmt.Println("Failed to create engine")
		return
	}
	defer pe.Destroy()

	fmt.Println("Engine Created Successfully")

	// Enable console logging for debugging
	pe.LogToConsole(true)

	// Define data requests to retrieve physiological values
	dataRequests := `{
		"DataRequest": [
			{"Category": "Physiology", "PropertyName": "HeartRate", "Unit": "1/min"},
			{"Category": "Physiology", "PropertyName": "RespirationRate", "Unit": "1/min"},
			{"Category": "Physiology", "PropertyName": "SystolicArterialPressure", "Unit": "mmHg"},
			{"Category": "Physiology", "PropertyName": "DiastolicArterialPressure", "Unit": "mmHg"},
			{"Category": "Physiology", "PropertyName": "OxygenSaturation"}
		]
	}`

	// Try to load from a state file
	stateFile := filepath.Join(dataDir, "states", "StandardMale@0s.json")
	fmt.Printf("Loading state from: %s\n", stateFile)

	err = pe.SerializeFromFile(stateFile, dataRequests, pulse.JSON)
	if err != nil {
		fmt.Printf("Failed to load state file: %v\n", err)
		fmt.Println("Note: Make sure the state file exists in the data directory")

		// Try initializing with a patient configuration instead
		fmt.Println("\nAttempting to initialize with patient configuration...")
		patientConfig := `{
			"Patient": {
				"Name": "StandardMale",
				"Sex": "Male",
				"Age": {"Value": 44.0, "Unit": "yr"},
				"Weight": {"Value": 170.0, "Unit": "lb"},
				"Height": {"Value": 71.0, "Unit": "in"}
			}
		}`
		err = pe.InitializeEngine(patientConfig, dataRequests, pulse.JSON)
		if err != nil {
			fmt.Printf("Failed to initialize engine: %v\n", err)
			return
		}
	}

	fmt.Println("Engine Initialized Successfully")

	// Get time step
	timeStep := pe.GetTimeStep("s")
	fmt.Printf("Time Step: %.4f seconds\n\n", timeStep)

	// Run simulation loop
	fmt.Println("Running simulation for 10 time steps...")
	fmt.Println("Time(s)\t\tHR(bpm)\t\tRR(bpm)\t\tSBP(mmHg)\tDBP(mmHg)\tSpO2")
	fmt.Println("------\t\t-------\t\t-------\t\t---------\t---------\t----")

	numDataRequests := 5
	for i := 0; i < 10; i++ {
		if !pe.AdvanceTimeStep() {
			fmt.Println("AdvanceTimeStep failed")
			break
		}

		// Pull data (returns slice of values matching data requests order)
		data := pe.PullData(numDataRequests)
		if data != nil && len(data) >= numDataRequests {
			simTime := float64(i+1) * timeStep
			fmt.Printf("%.2f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.2f\n",
				simTime, data[0], data[1], data[2], data[3], data[4])
		}
	}

	fmt.Println("\nSimulation Complete!")
}
