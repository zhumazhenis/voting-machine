package edu.nu.em.model.dao;

import edu.nu.em.model.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;


@Repository
public interface UserDAO extends JpaRepository<User, Integer>{
}
