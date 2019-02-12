package edu.nu.em.model.dao;

import edu.nu.em.model.Skill;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;


@Repository
public interface SkillDAO extends JpaRepository<Skill, Integer>{
}
