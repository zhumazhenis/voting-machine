package edu.nu.em.converter;

import edu.nu.em.model.dto.SkillDto;
import edu.nu.em.model.Skill;


public class SkillConverter {
  public static Skill dtoToEntity(SkillDto SkillDto) {
    Skill skill = new Skill();
    skill.setName(SkillDto.getSkillName());
    return skill;
  }

  public static SkillDto entityToDto(Skill skill) {
    return new SkillDto(skill.getId(), skill.getName());
  }
}
