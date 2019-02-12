package edu.nu.em.converter;

import edu.nu.em.dto.SkillDto;
import edu.nu.em.model.Skill;


public class SkillConverter {
  public static Skill dtoToEntity(SkillDto SkillDto) {
    Skill Skill = new Skill(SkillDto.getSkillName(), null);
    Skill.setSkillId(SkillDto.getSkillId());
    return Skill;
  }

  public static SkillDto entityToDto(Skill skill) {
    return new SkillDto(skill.getSkillId(), skill.getSkillName());
  }
}
