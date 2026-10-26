#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <bunseki/cbunseki.hpp>
#include <bunseki/sample_reader.hpp>
#include <bunseki/analysis.hpp>
#include <bunseki/transform.hpp>
#include <bunseki/buntools.hpp>

#include <cstddef>
#include <complex>
#include <vector>
#include <string>

namespace py = pybind11;

PYBIND11_MODULE(pybunseki, m){
    m.doc() = "Python bindings for Bunseki";

    m.def(
        "read_samples",
        [](const std::string& song_path, bool to_mono) -> py::object {
            auto result = read_samples(song_path, to_mono);

            if (!result){
                return py::none();
            }

            return py::cast(*result);
        },
        py::arg("song_path"),
        py::arg("to_mono") = true
    );

    py::class_<FrequencyBin>(m, "FrequencyBin")
        .def_readonly("frequency", &FrequencyBin::frequency)
        .def_readonly("magnitude", &FrequencyBin::magnitude);
        

    py::class_<Analysis>(m, "Analysis")
        // .def(py::init<const std::vector<float>&>(), py::arg("samples"))
        .def("rms", static_cast<float (Analysis::*)() const>(&Analysis::rms))
        .def("p2p_amplitude", &Analysis::p2p_amplitude)
        .def("peak_amplitude", &Analysis::peak_amplitude)
        .def("zero_crossing_rate", &Analysis::zero_crossing_rate)
        .def("silence_detection", &Analysis::silence_detection, py::arg("threshold") = 0.01f)
        .def(
            "rms_windows",
            &Analysis::rms_windows,
            py::arg("window_size") = STANDARD_WINDOW_SIZE,
            py::arg("hop_size") = STANDARD_HOP_SIZE
        )
        .def(
            "onset_detection",
            static_cast<std::vector<std::size_t> (Analysis::*)(float, int) const>
            (&Analysis::onset_detection),
            py::arg("threshold"),
            py::arg("min_spacing") = 2
        )
        .def(
            "onset_intervals",
            static_cast<std::vector<std::size_t> (Analysis::*)() const>
            (&Analysis::onset_intervals)
        )
        .def(
            "onset_bpm",
            static_cast<float (Analysis::*)() const>
            (&Analysis::onset_bpm)
        )
        .def(
            "auto_correlation",
            static_cast<std::vector<float> (Analysis::*)() const>
            (&Analysis::auto_correlation)
        )
        
        .def("peak_detection", &Analysis::peak_detection, py::arg("rms_values"))
        .def("lag_bpm", &Analysis::lag_bpm);

            
    py::class_<Transform>(m, "Transform")
        .def("dft", &Transform::dft)
        .def("fft", &Transform::fft)

        .def(
            "fft_frequencies",
            static_cast<std::vector<FrequencyBin> (Transform::*)() const>(&Transform::fft_frequencies)
        )
        
        .def(
            "stft",
            &Transform::stft,
            py::arg("window_size") = STANDARD_WINDOW_SIZE,
            py::arg("hop_size") = STANDARD_HOP_SIZE
        )

        .def(
            "stft_frequencies",
            &Transform::stft_frequencies,
            py::arg("window_size") = STANDARD_WINDOW_SIZE,
            py::arg("hop_size") = STANDARD_HOP_SIZE
        )

        .def(
            "spectral_centroid",
            static_cast<float (Transform::*)() const>(&Transform::spectral_centroid)
        );


    py::class_<CBunseki>(m, "CBunseki")
        .def(py::init<std::vector<float>>(), py::arg("samples"))

        .def_readonly("samples", &CBunseki::samples)
        .def_property_readonly("frame_count", &CBunseki::frame_count)

        .def_readonly("analysis", &CBunseki::analysis)
        .def_readonly("transform", &CBunseki::transform);

}


