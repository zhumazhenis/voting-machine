package edu.nu.em.model.dao;

import edu.nu.em.model.Person;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;


@Repository
public interface PersonDAO extends JpaRepository<Person, Integer> {
}
