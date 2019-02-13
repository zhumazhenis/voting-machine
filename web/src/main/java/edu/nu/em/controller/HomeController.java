package edu.nu.em.controller;

import edu.nu.em.model.Person;
import edu.nu.em.model.dao.PersonDAO;
import edu.nu.em.model.dao.SkillDAO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
public class HomeController {

  @Autowired
  PersonDAO personDAO;

  @Autowired
  SkillDAO skillDAO;

  @RequestMapping("/home")
  public String home() {
    return "index";
  }

  @RequestMapping("/test")
  @ResponseBody
  public String test() {

    Person person = new Person();
    person.setName("Talgat");
    personDAO.save(person);
    return skillDAO.findAll().get(0).getPerson().getName();
  }
}
