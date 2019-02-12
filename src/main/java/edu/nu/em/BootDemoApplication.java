package edu.nu.em;

import edu.nu.em.model.Skill;
import edu.nu.em.model.User;
import edu.nu.em.model.dao.UserDAO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import javax.annotation.PostConstruct;
import java.util.Arrays;

@SpringBootApplication
public class BootDemoApplication {
  @Autowired
  UserDAO userDAO;

  public static void main(String[] args) {
    SpringApplication.run(BootDemoApplication.class, args);
  }

  @PostConstruct
  public void setupDbWithData() {
    User user = new User("Ashish", null);
    user.setSkills(Arrays.asList(new Skill("java"), new Skill("js")));
    user = userDAO.save(user);
  }
}
