#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <bunseki/bunseki_all.hpp>
#include <vector>
#include <string>
#include <stdexcept>

namespace py = pybind11;

void bind_buntools(py::module_& m){
    auto buntools_m = m.def_submodule("buntools");

    buntools_m.def("is_power_of_two", &buntools::is_power_of_two, py::arg("number"));
    buntools_m.def("next_power_of_two", &buntools::next_power_of_two, py::arg("number"));
    buntools_m.def("reverse_bits", &buntools::reverse_bits, py::arg("number"), py::arg("bit_count"));

    buntools_m.def("samples_to_mono", &buntools::samples_to_mono, py::arg("samples"));
    buntools_m.def(
        "generate_sine_wave", &buntools::generate_sine_wave,
        py::arg("frequency"),
        py::arg("time"),
        py::arg("sample_rate") = 44100
    ); 

    buntools_m.def("read_dataset", &buntools::read_dataset, py::arg("csv_path"));

    auto constants_m = buntools_m.def_submodule("constants");

    constants_m.attr("SAMPLE_RATE") = SAMPLE_RATE;
    constants_m.attr("PI") = PI;
    constants_m.attr("STANDARD_WINDOW_SIZE") = STANDARD_WINDOW_SIZE;
    constants_m.attr("STANDARD_HOP_SIZE") = STANDARD_HOP_SIZE;

}

void bind_functions(py::module_& m){
    m.def(
        "read_samples",
        [](const std::string& song_path, bool to_mono) -> py::object {
            auto result = read_samples(song_path, to_mono);

            if (!result){
                throw std::runtime_error(result.error());
            }

            return py::cast(*result);
        },
        py::arg("song_path"),
        py::arg("to_mono") = true
    );
}

void bind_structures(py::module_& m){
    py::class_<FrequencyBin>(m, "FrequencyBin")
        .def_readonly("frequency", &FrequencyBin::frequency)
        .def_readonly("magnitude", &FrequencyBin::magnitude);
        
    py::class_<ChromaFeatures>(m, "ChromaFeatures")
        .def_readonly("chroma_windows", &ChromaFeatures::chroma_windows)
        .def_readonly("global_chroma", &ChromaFeatures::global_chroma)
        .def_readonly("mean_chroma", &ChromaFeatures::mean_chroma)
        .def_readonly("chroma_variance", &ChromaFeatures::chroma_variance);
}

void bind_analysis(py::module_& m){
    py::class_<Analysis>(m, "Analysis")
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

}

void bind_transform(py::module_& m){
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
        )

        .def("global_chroma", &Transform::global_chroma)

        .def(
            "chroma_windows",
            static_cast<std::vector<std::array<float, 12>>
            (Transform::*)() const>(&Transform::chroma_windows)
        )

        .def(
            "chroma_windows",
            static_cast<std::vector<std::array<float, 12>>
            (Transform::*)(const std::vector<std::vector<FrequencyBin>>&) const>(&Transform::chroma_windows),
            py::arg("stft_freqs")
        )

        .def(
            "mean_chroma",
            static_cast<std::array<float, 12> (Transform::*)() const>(&Transform::mean_chroma)
        )

        .def(
            "mean_chroma",
            static_cast<std::array<float, 12> (Transform::*)(const std::vector<std::array<float, 12>>&) const>
                (&Transform::mean_chroma),
            py::arg("chroma_vector")
        )

        .def(
            "chroma_variance",
            static_cast<std::array<float, 12> (Transform::*)() const>(&Transform::chroma_variance)
        )

        .def(
            "chroma_variance",
            static_cast<std::array<float, 12> (Transform::*)(const std::vector<std::array<float, 12>>&) const>
                (&Transform::chroma_variance),
            py::arg("chroma_vector")
        )

        .def(
            "chroma_features",
            &Transform::chroma_features
        );

    
}

void bind_cbunseki(py::module_& m){
    py::class_<CBunseki>(m, "CBunseki")
        .def(py::init<std::vector<float>>(), py::arg("samples"))

        .def_readonly("samples", &CBunseki::samples)

        .def_property_readonly("frame_count", &CBunseki::frame_count)

        .def_readonly("analysis", &CBunseki::analysis, py::return_value_policy::reference_internal)
        .def_readonly("transform", &CBunseki::transform, py::return_value_policy::reference_internal);
        // esse py::return_value_policy::reference_internal mantém o objeto pai vivo enquanto os filhos estiverem sendo usados.
        // isso garante que não haja problemas de memória decorrentes de todos usarem referências para o mesmo objeto samples
}

void bind_metadata(py::module_& m){
    py::class_<CoverArt>(m, "CoverArt")
        .def_readonly("data", &CoverArt::data)
        .def_readonly("file_type", &CoverArt::file_type)
        .def_readonly("description", &CoverArt::description);
        
    py::class_<Metadata> metadata(m, "Metadata");

    py::enum_<Metadata::Status>(metadata, "Status")
        .value("NOT_LOADED", Metadata::Status::not_loaded)
        .value("SUCCESS", Metadata::Status::success)
        .value("FILE_NOT_FOUND", Metadata::Status::file_not_found)
        .value("NO_TAGS_FOUND", Metadata::Status::no_tags_found);

        
    metadata
        .def(py::init<std::string>(), py::arg("song_path"))
        .def("load_metadata", &Metadata::load_metadata, py::return_value_policy::reference_internal)
        .def("get_status", &Metadata::get_status)
        .def("check_status", &Metadata::check_status, py::arg("stat"))
        .def("discovered_tags", &Metadata::discovered_tags)
        .def(
            "get_tags",
            &Metadata::get_tags,
            py::return_value_policy::reference_internal
        )
        .def("__bool__", &Metadata::operator bool)
        .def(
            "__iter__",
            [](const Metadata& self){
                return py::make_iterator(self.begin(), self.end());
            },
            py::keep_alive<0, 1>() // mantém o Metadata vivo enquanto o iterador estiver sendo usado
        )
        .def("__getitem__", &Metadata::operator[])
        .def("__contains__", &Metadata::contains)
        .def(
            "__getattr__",
            [](const Metadata& self, const std::string& item){
                try {
                    return self[item];
                }
                catch (const std::out_of_range&){
                    throw py::attribute_error(
                        "Metadata has no attribute '" + item + "'."
                    );
                }
            }
        )
        .def(
            "get",
            [](const Metadata& self, const std::string& item, py::object default_value) -> py::object {
                try {
                    return py::cast(self[item]);
                }
                catch (const std::out_of_range&){
                    return default_value;
                }
            },
            py::arg("item"),
            py::arg("default_value") = py::none()
        )
        .def("get_cover", &Metadata::get_cover);

    
}

PYBIND11_MODULE(_pybunseki, m){
    m.doc() = "Python bindings for Bunseki";

    bind_buntools(m);
    bind_functions(m);
    bind_structures(m);

    bind_analysis(m);
    bind_transform(m);
    bind_cbunseki(m);

    bind_metadata(m);

}


