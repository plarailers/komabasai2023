from typing import Any
from .components import Joint, Junction, Section


class RailwayConfig:
    """
    路線の設定
    """

    junctions: dict["Junction", "JunctionConfig"]
    sections: dict["Section", "SectionConfig"]

    def __init__(self) -> None:
        self.junctions = {}
        self.sections = {}

    def define_junctions(self, *junction_tuples: tuple["Junction"]) -> None:
        """
        分岐・合流点を一斉に定義する。

        形式: `(ID,)`
        """
        for (junction_id,) in junction_tuples:
            self.junctions[junction_id] = JunctionConfig()

    def define_sections(
        self, *section_tuples: tuple["Section", "Junction", "Joint", "Junction", "Joint", float]
    ) -> None:
        """
        区間を一斉に定義する。

        形式: `(ID, j0のID, j0との接続方法, j1のID, j1との接続方法, 長さ[mm])`
        """
        for section_id, junction_0_id, junction_0_joint, junction_1_id, junction_1_joint, length in section_tuples:
            self.junctions[junction_0_id].add_section(junction_0_joint, section_id)
            self.junctions[junction_1_id].add_section(junction_1_joint, section_id)
            self.sections[section_id] = SectionConfig(
                junction_0=junction_0_id,
                junction_1=junction_1_id,
                length=length,
            )

    def to_json(self) -> Any:
        data: Any = {
            "junctions": dict((id, {}) for id, j in self.junctions.items()),
            "sections": dict((id, {}) for id, s in self.sections.items()),
        }
        return data


class JunctionConfig:
    _sections: dict["Joint", "Section"]

    def __init__(
        self,
    ) -> None:
        self._sections = {}

    def add_section(self, joint: "Joint", section: "Section") -> None:
        self._sections[joint] = section


class SectionConfig:
    _length: float
    _junction_0: "Junction"
    _junction_1: "Junction"

    def __init__(
        self,
        *,
        junction_0: "Junction",
        junction_1: "Junction",
        length: float,
    ) -> None:
        self._length = length
        self._junction_0 = junction_0
        self._junction_1 = junction_1


def init_config() -> RailwayConfig:
    config = RailwayConfig()

    j0a = Junction("j0a")
    j0b = Junction("j0b")
    j1a = Junction("j1a")
    j1b = Junction("j1b")
    j2a = Junction("j2a")
    j2b = Junction("j2b")
    j3a = Junction("j3a")
    j3b = Junction("j3b")

    s00 = Section("s00")
    s01 = Section("s01")
    s02 = Section("s02")
    s03 = Section("s03")
    s04 = Section("s04")
    s05 = Section("s05")
    s06 = Section("s06")
    s07 = Section("s07")
    s08 = Section("s08")
    s09 = Section("s09")
    s10 = Section("s10")
    s11 = Section("s11")

    config.define_junctions(
        (j0a,),
        (j0b,),
        (j1a,),
        (j1b,),
        (j2a,),
        (j2b,),
        (j3a,),
        (j3b,),
    )

    config.define_sections(
        (s00, j0a, Joint.CONVERGING, j0b, Joint.THROUGH, 100),
        (s01, j0b, Joint.CONVERGING, j1b, Joint.CONVERGING, 100),
        (s02, j1b, Joint.THROUGH, j2b, Joint.THROUGH, 100),
        (s03, j2b, Joint.CONVERGING, j3b, Joint.CONVERGING, 100),
        (s04, j3b, Joint.THROUGH, j3a, Joint.CONVERGING, 100),
        (s05, j3a, Joint.THROUGH, j2a, Joint.THROUGH, 100),
        (s06, j2a, Joint.CONVERGING, j1a, Joint.CONVERGING, 100),
        (s07, j1a, Joint.THROUGH, j0a, Joint.THROUGH, 100),
        (s08, j0a, Joint.DIVERGING, j0b, Joint.DIVERGING, 100),
        (s09, j1a, Joint.DIVERGING, j1b, Joint.DIVERGING, 100),
        (s10, j2a, Joint.DIVERGING, j2b, Joint.DIVERGING, 100),
        (s11, j3a, Joint.DIVERGING, j3b, Joint.DIVERGING, 100),
    )

    return config
