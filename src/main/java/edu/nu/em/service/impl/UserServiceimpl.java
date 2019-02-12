package edu.nu.em.service.impl;

import edu.nu.em.converter.UserConverter;
import edu.nu.em.dto.UserDto;
import edu.nu.em.model.dao.UserDAO;
import edu.nu.em.service.UserService;

import java.util.List;
import java.util.stream.Collectors;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;


@Service
public class UserServiceimpl implements UserService {
	@Autowired
	UserDAO userDAO;

	@Override
	public UserDto getUserById(Integer userId) {
		return UserConverter.entityToDto(userDAO.getOne(userId));
	}

	@Override
	public void saveUser(UserDto userDto) {
		userDAO.save(UserConverter.dtoToEntity(userDto));
	}

	@Override
	public List<UserDto> getAllUsers() {
		return userDAO.findAll().stream().map(UserConverter::entityToDto).collect(Collectors.toList());
	}
}
