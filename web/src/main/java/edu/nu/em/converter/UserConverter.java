package edu.nu.em.converter;

import edu.nu.em.model.dto.PersonDto;
import edu.nu.em.model.Person;

import java.util.stream.Collectors;


public class UserConverter {
  public static Person dtoToEntity(PersonDto personDto) {
    Person person = new Person();
    person.setName(personDto.getUserName());
    person.setSkills(personDto.getSkillDtos().stream().map(SkillConverter::dtoToEntity).collect(Collectors.toList()));
    return person;
  }

  public static PersonDto entityToDto(Person person) {
    PersonDto personDto = new PersonDto(person.getId(), person.getName(), null);
    personDto.setSkillDtos(person.getSkills().stream().map(SkillConverter::entityToDto).collect(Collectors.toList()));
    return personDto;
  }
}
