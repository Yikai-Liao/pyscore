//
// Created by lyk on 23-9-20.
//
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include "Score.hpp"

namespace py = pybind11;
using namespace score;

PYBIND11_MAKE_OPAQUE(std::vector<Track>)
PYBIND11_MAKE_OPAQUE(std::vector<Note>)
PYBIND11_MAKE_OPAQUE(std::vector<ControlChange>)
PYBIND11_MAKE_OPAQUE(std::vector<TimeSignature>)
PYBIND11_MAKE_OPAQUE(std::vector<KeySignature>)
PYBIND11_MAKE_OPAQUE(std::vector<Tempo>)
PYBIND11_MAKE_OPAQUE(std::unordered_map<uint8_t, std::vector<ControlChange>>)

PYBIND11_MODULE(pyscore, m) {

    py::bind_vector<std::vector<Track>>(m, "TrackList")
        .def("__repr__", [](const std::vector<Track> &self) {
            return "<TrackList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_vector<std::vector<Note>>(m, "NoteList")
        .def("__repr__", [](const std::vector<Note> &self) {
            return "<NoteList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_vector<std::vector<ControlChange>>(m, "ControlChangeList")
        .def("__repr__", [](const std::vector<ControlChange> &self) {
            return "<ControlChangeList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_vector<std::vector<TimeSignature>>(m, "TimeSignatureList")
        .def("__repr__", [](const std::vector<TimeSignature> &self) {
            return "<TimeSignatureList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_vector<std::vector<KeySignature>>(m, "KeySignatureList")
        .def("__repr__", [](const std::vector<KeySignature> &self) {
            return "<KeySignatureList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_vector<std::vector<Tempo>>(m, "TempoList")
        .def("__repr__", [](const std::vector<Tempo> &self) {
            return "<TempoList length=" + std::to_string(self.size()) + ">";
        });
    py::bind_map<std::unordered_map<uint8_t, std::vector<ControlChange>>>(m, "ControlMap")
        .def("__repr__", [](const std::unordered_map<uint8_t, std::vector<ControlChange>> &self) {
            size_t control_num = 0;
            for (const auto& vec: self) control_num += vec.second.size();
            return "<ControlMap control_types=" + std::to_string(self.size())
                   + ", event_num=" + std::to_string(self.size()) + ">";
        });

    py::class_<Score>(m, "Score")
        .def(py::init<>())
        .def(py::init(&Score::from_file))
        .def_property_readonly_static("from_file", [](const py::object &) {
            return py::cpp_function([](std::string &x) { return Score::from_file(x); });
        })  // binding class method in an erratic way: https://github.com/pybind/pybind11/issues/1693
        .def("sort", &Score::sort)
        .def("shift_pitch", &Score::shift_pitch)
        .def("shift_time", &Score::shift_time)
        .def("shift_velocity", &Score::shift_velocity)
        .def_readwrite("tracks", &Score::tracks)
        .def_readwrite("time_signatures", &Score::time_signatures)
        .def_readwrite("key_signatures", &Score::key_signatures)
        .def_readwrite("tempos", &Score::tempos)
        .def("__repr__", [](const Score &self) {
            return "<Score track_num=" + std::to_string(self.tracks.size()) + ">";
        });

    py::class_<Track>(m, "Track")
        .def(py::init<>())
        .def("sort", &Track::sort)
        .def("shift_pitch", &Track::shift_pitch)
        .def("shift_time", &Track::shift_time)
        .def("shift_velocity", &Track::shift_velocity)
        .def_readwrite("name", &Track::name)
        .def_readwrite("program", &Track::program)
        .def_readwrite("is_drum", &Track::is_drum)
        .def_readwrite("notes", &Track::notes)
        .def_readwrite("controls", &Track::controls)
        .def("__repr__", [](const Track &self) {
            return "<Track name=" + self.name +
                   ", program=" + std::to_string((int) self.program) +
                   ", is_drum=" + (self.is_drum ? "True" : "False") +
                   ", note_num=" + std::to_string(self.notes.size()) + ">";
        });

    py::class_<Note>(m, "Note")
        .def(py::init<float, float, int8_t, int8_t>())
        .def_readwrite("start", &Note::start)
        .def_readwrite("duration", &Note::duration)
        .def_readwrite("pitch", &Note::pitch)
        .def_readwrite("velocity", &Note::velocity)
        .def("__repr__", [](const Note &self) {
            return "<Note start=" + std::to_string(self.start) +
                   ", dur=" + std::to_string(self.duration) +
                   ", pitch=" + std::to_string((int) self.pitch) +
                   ", vel=" + std::to_string((int) self.velocity) + ">";
        });

    py::class_<TimeSignature>(m, "TimeSignature")
        .def(py::init<float, uint8_t, uint8_t>())
        .def_readwrite("time", &score::TimeSignature::time)
        .def_readwrite("numerator", &score::TimeSignature::numerator)
        .def_readwrite("denominator", &score::TimeSignature::denominator)
        .def("__repr__", [](TimeSignature &self) {
            return "<TimeSignature time=" + std::to_string(self.time) +
                   ", numerator=" + std::to_string((int) self.numerator) +
                   ", denominator=" + std::to_string((int) self.denominator) + ">";
        });

    py::class_<KeySignature>(m, "KeySignature")
        .def(py::init<float, int8_t, uint8_t>())
        .def_readwrite("time", &score::KeySignature::time)
        .def_readwrite("key", &score::KeySignature::key)
        .def_readwrite("tonality", &score::KeySignature::tonality)
        .def("__repr__", &score::KeySignature::to_string);

    py::class_<score::ControlChange>(m, "ControlChange")
        .def(py::init<float, uint8_t>())
        .def_readwrite("time", &score::ControlChange::time)
        .def_readwrite("value", &score::ControlChange::value)
        .def("__repr__", [](score::ControlChange &self) {
            return "<ControlChange time=" + std::to_string(self.time) +
                   ", value=" + std::to_string((int) self.value) + ">";
        });

    py::class_<score::Tempo>(m, "Tempo")
        .def(py::init<float, uint32_t>())
        .def_readwrite("time", &score::Tempo::time)
        .def_readwrite("tempo", &score::Tempo::qpm)
        .def("__repr__", [](score::Tempo &self) {
            return "<Tempo time=" + std::to_string(self.time) +
                   ", qpm=" + std::to_string(self.qpm) + ">";
        });


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}