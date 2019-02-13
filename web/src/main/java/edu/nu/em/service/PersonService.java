package edu.nu.em.service;

import edu.nu.em.model.dto.PersonDto;

import java.util.List;


public interface PersonService {

  PersonDto getUserById(Integer userId);

  void saveUser(PersonDto personDto);

  List<PersonDto> getAllUsers();

}
