package edu.nu.em.service.impl;

import edu.nu.em.converter.UserConverter;
import edu.nu.em.model.dto.PersonDto;
import edu.nu.em.model.dao.PersonDAO;
import edu.nu.em.service.PersonService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.stream.Collectors;


@Service
public class PersonServiceimpl implements PersonService {

  @Autowired
  PersonDAO personDAO;

  @Override
  public PersonDto getUserById(Integer userId) {
    return UserConverter.entityToDto(personDAO.getOne(userId));
  }

  @Override
  public void saveUser(PersonDto personDto) {
    personDAO.save(UserConverter.dtoToEntity(personDto));
  }

  @Override
  public List<PersonDto> getAllUsers() {
    return personDAO.findAll().stream().map(UserConverter::entityToDto).collect(Collectors.toList());
  }
}
