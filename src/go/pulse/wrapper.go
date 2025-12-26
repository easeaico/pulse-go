package pulse

/*
#include "bridge.h"
*/
import "C"
import (
	"errors"
	"unsafe"
)

// PulseEngine represents an instance of the physiology engine.
type PulseEngine struct {
	thunk *C.PhysiologyEngineThunk
}

// Global initialization of the Pulse library.
func Initialize() {
	C.PulseInitialize()
}

// Global deinitialization of the Pulse library.
func Deinitialize() {
	C.PulseDeinitialize()
}

// Build information.
func Version() string {
	var cStr *C.char
	C.PulseVersion(&cStr)
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr)
}

func Hash() string {
	var cStr *C.char
	C.PulseHash(&cStr)
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr)
}

// ExecuteScenario executes a scenario with the given options.
func ExecuteScenario(sceOpts string, format SerializationFormat) error {
	cSceOpts := C.CString(sceOpts)
	defer C.free(unsafe.Pointer(cSceOpts))
	if C.ExecuteScenario(cSceOpts, C.int(format)) == 0 {
		return errors.New("ExecuteScenario failed")
	}
	return nil
}

// AreCompatibleUnits checks if two unit strings are compatible.
func AreCompatibleUnits(fromUnit, toUnit string) bool {
	cFromUnit := C.CString(fromUnit)
	defer C.free(unsafe.Pointer(cFromUnit))
	cToUnit := C.CString(toUnit)
	defer C.free(unsafe.Pointer(cToUnit))
	return C.AreCompatibleUnits(cFromUnit, cToUnit) != 0
}

// ConvertValue converts a value from one unit to another.
func ConvertValue(value float64, fromUnit, toUnit string) float64 {
	cFromUnit := C.CString(fromUnit)
	defer C.free(unsafe.Pointer(cFromUnit))
	cToUnit := C.CString(toUnit)
	defer C.free(unsafe.Pointer(cToUnit))
	return float64(C.ConvertValue(C.double(value), cFromUnit, cToUnit))
}

// NewPulseEngine creates a new engine instance.
func NewPulseEngine(model EngineModel, dataDir string) *PulseEngine {
	cDataDir := C.CString(dataDir)
	defer C.free(unsafe.Pointer(cDataDir))
	thunk := C.Allocate(C.int(model), cDataDir)
	return &PulseEngine{thunk: thunk}
}

// Destroy cleans up the engine instance.
func (pe *PulseEngine) Destroy() {
	if pe.thunk != nil {
		C.Deallocate(pe.thunk)
		pe.thunk = nil
	}
}

func (pe *PulseEngine) Clear() {
	C.Clear(pe.thunk)
}

func (pe *PulseEngine) SerializeFromFile(filename string, dataRequests string, format SerializationFormat) error {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))
	cDataRequests := C.CString(dataRequests)
	defer C.free(unsafe.Pointer(cDataRequests))

	if C.SerializeFromFile(pe.thunk, cFilename, cDataRequests, C.int(format)) == 0 {
		return errors.New("SerializeFromFile failed")
	}
	return nil
}

func (pe *PulseEngine) SerializeToFile(filename string) error {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))
	if C.SerializeToFile(pe.thunk, cFilename) == 0 {
		return errors.New("SerializeToFile failed")
	}
	return nil
}

func (pe *PulseEngine) SerializeFromString(state string, dataRequests string, format SerializationFormat) error {
	cState := C.CString(state)
	defer C.free(unsafe.Pointer(cState))
	cDataRequests := C.CString(dataRequests)
	defer C.free(unsafe.Pointer(cDataRequests))

	if C.SerializeFromString(pe.thunk, cState, cDataRequests, C.int(format)) == 0 {
		return errors.New("SerializeFromString failed")
	}
	return nil
}

func (pe *PulseEngine) SerializeToString(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.SerializeToString(pe.thunk, C.int(format), &cStr) == 0 {
		return "", errors.New("SerializeToString failed")
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) InitializeEngine(patientConfiguration string, dataRequests string, format SerializationFormat) error {
	cPatientConfig := C.CString(patientConfiguration)
	defer C.free(unsafe.Pointer(cPatientConfig))
	cDataRequests := C.CString(dataRequests)
	defer C.free(unsafe.Pointer(cDataRequests))

	if C.InitializeEngine(pe.thunk, cPatientConfig, cDataRequests, C.int(format)) == 0 {
		return errors.New("InitializeEngine failed")
	}
	return nil
}

func (pe *PulseEngine) GetInitialPatient(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.GetInitialPatient(pe.thunk, C.int(format), &cStr) == 0 {
		return "", errors.New("GetInitialPatient failed")
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) GetConditions(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.GetConditions(pe.thunk, C.int(format), &cStr) == 0 {
		return "", errors.New("GetConditions failed")
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) LogToConsole(b bool) {
	val := C.int(0)
	if b {
		val = 1
	}
	C.LogToConsole(pe.thunk, val)
}

func (pe *PulseEngine) KeepLogMessages(keep bool) {
	val := C.int(0)
	if keep {
		val = 1
	}
	C.KeepLogMessages(pe.thunk, val)
}

func (pe *PulseEngine) SetLogFilename(filename string) {
	cFilename := C.CString(filename)
	defer C.free(unsafe.Pointer(cFilename))
	C.SetLogFilename(pe.thunk, cFilename)
}

func (pe *PulseEngine) PullLogMessages(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.PullLogMessages(pe.thunk, C.int(format), &cStr) == 0 {
		return "", nil // No log messages is not an error
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) KeepEventChanges(keep bool) {
	val := C.int(0)
	if keep {
		val = 1
	}
	C.KeepEventChanges(pe.thunk, val)
}

func (pe *PulseEngine) PullEvents(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.PullEvents(pe.thunk, C.int(format), &cStr) == 0 {
		return "", nil // No events is not an error
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) PullActiveEvents(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.PullActiveEvents(pe.thunk, C.int(format), &cStr) == 0 {
		return "", nil // No active events is not an error
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) GetPatientAssessment(assessmentType int, format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.GetPatientAssessment(pe.thunk, C.int(assessmentType), C.int(format), &cStr) == 0 {
		return "", errors.New("GetPatientAssessment failed")
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) ProcessActions(actions string, format SerializationFormat) error {
	cActions := C.CString(actions)
	defer C.free(unsafe.Pointer(cActions))
	if C.ProcessActions(pe.thunk, cActions, C.int(format)) == 0 {
		return errors.New("ProcessActions failed")
	}
	return nil
}

func (pe *PulseEngine) PullActiveActions(format SerializationFormat) (string, error) {
	var cStr *C.char
	if C.PullActiveActions(pe.thunk, C.int(format), &cStr) == 0 {
		return "", nil // No active actions is not an error
	}
	defer C.free(unsafe.Pointer(cStr))
	return C.GoString(cStr), nil
}

func (pe *PulseEngine) AdvanceTimeStep() bool {
	return C.AdvanceTimeStep(pe.thunk) != 0
}

func (pe *PulseEngine) GetTimeStep(unit string) float64 {
	cUnit := C.CString(unit)
	defer C.free(unsafe.Pointer(cUnit))
	return float64(C.GetTimeStep(pe.thunk, cUnit))
}

// PullData returns simulation data values.
// The count parameter specifies how many values to retrieve (based on data requests).
func (pe *PulseEngine) PullData(count int) []float64 {
	ptr := C.PullData(pe.thunk)
	if ptr == nil {
		return nil
	}
	// Create a slice backed by the C array
	// Note: The slice is valid only until the next PullData call or engine destruction
	return (*[1 << 30]float64)(unsafe.Pointer(ptr))[:count:count]
}
