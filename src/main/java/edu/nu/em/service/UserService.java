package edu.nu.em.service;

import java.util.List;

import edu.nu.em.dto.UserDto;


public interface UserService {

  UserDto getUserById(Integer userId);

  void saveUser(UserDto userDto);

  List<UserDto> getAllUsers();

}
