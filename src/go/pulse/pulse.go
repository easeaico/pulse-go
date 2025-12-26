package pulse

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo LDFLAGS: -L${SRCDIR}/../../../bin -lPulseC
#include "bridge.h"
*/
import "C"

// SerializationFormat defines the format for data serialization.
type SerializationFormat int

const (
	JSON SerializationFormat = 0
	// BINARY format if supported
)

// EngineModel defines the type of physiology model.
type EngineModel int

const (
	HumanAdultWholeBody    EngineModel = 0
	HumanAdultVentilation  EngineModel = 1
	HumanAdultHemodynamics EngineModel = 2
)

// PatientAssessmentType defines types of patient assessments.
type PatientAssessmentType int

const (
	CompleteBloodCount          PatientAssessmentType = 0
	ComprehensiveMetabolicPanel PatientAssessmentType = 1
	Urinalysis                  PatientAssessmentType = 2
	PulmonaryFunctionTest       PatientAssessmentType = 3
	ArterialBloodGasTest        PatientAssessmentType = 4
	PatientAssessmentTypeCount  PatientAssessmentType = 5
)
