
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

#include "analysis.hpp"
#include "sample_reader.hpp"
#include "transform.hpp"

#include <string>

namespace py = pybind11;

PYBIND11_MODULE(_bunseki, m){
    py::class_<Analysis>(m, "Analysis")
        .def(py::init<std::vector<float>, std::uint64_t>(), py::arg("samples"), py::arg("frame_count"))
        .def_readwrite("samples", &Analysis::samples)
        .def_readwrite("frame_count", &Analysis::frame_count)
        .def("ptp_amplitude", &Analysis::ptp_amplitude)
        .def("peak_amplitude", &Analysis::peak_amplitude)
        .def("rms", &Analysis::rms)
        .def("dc_offset", &Analysis::dc_offset)
        .def("crest_factor", &Analysis::crest_factor)
        .def("zero_crossing_rate", &Analysis::zero_crossing_rate)
        .def(
            "silence_detection",
            &Analysis::silence_detection,
            py::arg("window_size_ms") = 20,
            py::arg("threshold") = 0.01f
        );
    
        py::class_<Transform>(m, "Transform")
            .def(py::init<std::vector<float>, std::uint64_t>(), py::arg("samples"), py::arg("frame_count"))
            .def_readwrite("samples", &Transform::samples)
            .def_readwrite("frame_count", &Transform::frame_count)
            .def("dft", &Transform::dft)
            .def("fft", static_cast<std::vector<Complex>(Transform::*)()>(&Transform::fft))
            .def("mono_samples", &Transform::mono_samples);

    m.def("read_samples", [](const std::string& song_path) -> py::object {
        auto result = read_samples(song_path);

        if (!result){
            return py::none();
        }

        return py::cast(*result);

    }, py::arg("song_path"));
        
    }


