package edu.nu.em.controller;

import edu.nu.em.model.dto.PersonDto;
import edu.nu.em.service.PersonService;
import edu.nu.em.utils.Constants;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;


@RequestMapping("/user")
@RestController
public class PersonController {
  @Autowired
  PersonService personService;

  @RequestMapping(Constants.GET_USER_BY_ID)
  public PersonDto getUserById(@PathVariable Integer userId) {
    return personService.getUserById(userId);
  }

  @RequestMapping(Constants.GET_ALL_USERS)
  public List<PersonDto> getAllUsers() {
    return personService.getAllUsers();
  }

  @RequestMapping(value = Constants.SAVE_USER, method = RequestMethod.POST)
  public void saveUser(@RequestBody PersonDto personDto) {
    personService.saveUser(personDto);
  }
}
